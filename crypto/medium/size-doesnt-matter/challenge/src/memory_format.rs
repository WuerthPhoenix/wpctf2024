use libflate::lz77::{self, Code, Lz77Encode, Sink};

pub fn compress(data: &[u8]) -> Vec<u8> {
    let mut buffer = Lz77Sink::new();
    let mut encoder = lz77::DefaultLz77Encoder::with_window_size(1024);
    encoder.encode(data, &mut buffer);
    encoder.flush(&mut buffer);
    buffer.finish()
}

pub fn uncompress(data: &[u8]) -> Result<Vec<u8>, DecodingError> {
    let mut decoder = lz77::Lz77Decoder::new();
    let data = Lz77Source::try_new(&data)?;

    for code in data.iter() {
        if decoder.decode(code?).is_err() {
            return Err(DecodingError);
        };
    }

    Ok(decoder.buffer().to_vec())
}

#[derive(Debug)]
pub struct DecodingError;

struct Lz77Sink {
    length: u32,
    buffer: BitStreamWriter,
}

impl Lz77Sink {
    fn new() -> Self {
        Self {
            length: 0,
            buffer: BitStreamWriter::default(),
        }
    }

    fn finish(&self) -> Vec<u8> {
        let mut buffer = self.length.to_be_bytes().to_vec();
        buffer.extend_from_slice(self.buffer.as_bytes());
        buffer
    }
}

impl Sink for Lz77Sink {
    fn consume(&mut self, code: libflate::lz77::Code) {
        self.length += 1;
        match code {
            libflate::lz77::Code::Literal(lit) => {
                self.buffer.write_bool(false);
                self.buffer.write_u8(lit);
            }
            libflate::lz77::Code::Pointer {
                length,
                backward_distance,
            } => {
                self.buffer.write_bool(true);
                self.buffer.write_u16(length);
                self.buffer.write_u16(backward_distance);
            }
        }
    }
}

struct Lz77Source<'data> {
    length: u32,
    data: &'data [u8],
}

impl<'a> Lz77Source<'a> {
    fn try_new(data: &'a [u8]) -> Result<Lz77Source<'a>, DecodingError> {
        let [a, b, c, d, tail @ ..] = data else {
            return Err(DecodingError);
        };
        let length = u32::from_be_bytes([*a, *b, *c, *d]);
        Ok(Self { length, data: tail })
    }
}

impl Lz77Source<'_> {
    fn iter(&self) -> Lz77Iter<'_> {
        Lz77Iter {
            length: self.length,
            data: BitStreamReader {
                offset: 0,
                bytes: self.data,
            },
        }
    }
}

struct Lz77Iter<'data> {
    length: u32,
    data: BitStreamReader<'data>,
}

impl Lz77Iter<'_> {
    fn try_next(&mut self) -> Option<Code> {
        if self.data.read_bool()? {
            let length = self.data.read_u16()?;
            let backward_distance = self.data.read_u16()?;
            Some(Code::Pointer {
                length,
                backward_distance,
            })
        } else {
            Some(Code::Literal(self.data.read_u8()?))
        }
    }
}

impl Iterator for Lz77Iter<'_> {
    type Item = Result<Code, DecodingError>;

    fn next(&mut self) -> Option<Self::Item> {
        if self.length == 0 {
            return None;
        }
        self.length -= 1;
        match self.try_next() {
            Some(code) => Some(Ok(code)),
            None => Some(Err(DecodingError)),
        }
    }
}

#[derive(Default)]
struct BitStreamWriter {
    length: usize,
    data: Vec<u8>,
}

impl BitStreamWriter {
    fn write_bool(&mut self, b: bool) {
        let byte = self.length / 8;
        let bit = self.length % 8;

        if byte >= self.data.len() {
            self.data.push(0);
        }

        self.data[byte] |= (b as u8) << (7 - bit);
        self.length += 1;
    }

    fn write_u8(&mut self, b: u8) {
        let mut i = 8;
        while i > 0 {
            i -= 1;
            let b = (b & 1 << i) != 0;
            self.write_bool(b);
        }
    }

    fn write_u16(&mut self, b: u16) {
        let buffer = b.to_be_bytes();
        self.write_u8(buffer[0]);
        self.write_u8(buffer[1]);
    }

    fn as_bytes(&self) -> &[u8] {
        let len = (self.length / 8) + (self.length % 8 > 0) as usize;
        &self.data[..len]
    }
}

struct BitStreamReader<'data> {
    offset: usize,
    bytes: &'data [u8],
}

impl BitStreamReader<'_> {
    fn read_bool(&mut self) -> Option<bool> {
        let byte = self.offset / 8;
        let bit = self.offset % 8;

        let byte = *self.bytes.get(byte)?;
        let mask = 1 << (7 - bit);
        let result = (byte & mask) != 0;
        self.offset += 1;
        Some(result)
    }

    fn read_u8(&mut self) -> Option<u8> {
        let mut i = 8;
        let mut result = 0;
        while i > 0 {
            i -= 1;
            let bit = self.read_bool()?;
            result |= (bit as u8) << i;
        }

        Some(result)
    }

    fn read_u16(&mut self) -> Option<u16> {
        let a = self.read_u8()?;
        let b = self.read_u8()?;
        Some(u16::from_be_bytes([a, b]))
    }
}

#[cfg(test)]
mod tests {
    use crate::memory_format::{Lz77Sink, Lz77Source};
    use core::str;
    use libflate::lz77::{self, Lz77Encode};

    #[test]
    fn should_compress_and_decompress() {
        let text = "Hello, World! Hello, beautiful World";

        let mut encoder = lz77::DefaultLz77Encoder::with_window_size(1024);
        let mut buffer = Lz77Sink::new();

        encoder.encode(text.as_bytes(), &mut buffer);
        encoder.flush(&mut buffer);
        let data = buffer.finish();

        let mut decoder = lz77::Lz77Decoder::new();
        let data = Lz77Source::try_new(&data).unwrap();
        let mut iter = data.iter();

        while let Some(code) = iter.next() {
            let code = code.unwrap();
            decoder.decode(code).unwrap();
        }

        let data = decoder.buffer();
        assert_eq!(str::from_utf8(data).unwrap(), text);
    }

    #[test]
    fn should_fail_on_wrong_length() {
        let data = [0, 0, 0, 25, 0, 0];

        let source = Lz77Source::try_new(&data).unwrap();
        let mut iter = source.iter();

        assert!(iter.next().unwrap().is_ok());
        assert!(iter.next().unwrap().is_err());
    }
}
