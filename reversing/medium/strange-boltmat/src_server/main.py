#!/usr/bin/env python3

import logging
import random
import sys
import xml.etree.ElementTree as ET
from dataclasses import dataclass


@dataclass
class MetricData:
    timestamp: str
    registered_keypress: str


def read_till_0() -> str:
    data = bytearray()

    while True:
        byte = sys.stdin.buffer.read(1)
        if not byte or byte == b"\0":
            break
        data += byte

    return data.decode("utf-8")


def get_metric(data: str) -> MetricData:
    root = ET.fromstring(data)

    timestamp = root.find("timestamp").text
    registered_keypress = root.find("keypress").text

    return MetricData(timestamp, registered_keypress)


def response_filling_buffer(data: MetricData) -> ET.Element:
    root = ET.Element("ack")

    status = ET.Element("status")
    status.text = "FillingBuffer"

    timestamp = ET.Element("lastTimestamp")
    timestamp.text = data.timestamp

    registered_keypress = ET.Element("registeredKeypress")
    registered_keypress.text = data.registered_keypress

    root.append(registered_keypress)
    root.append(timestamp)
    root.append(status)

    return root


def response_flag_char(data: MetricData, flag_char: str) -> ET.Element:
    root = ET.Element("ack")

    status = ET.Element("status")
    status.text = "Success"

    timestamp = ET.Element("lastTimestamp")
    timestamp.text = data.timestamp

    registered_keypress = ET.Element("registeredKeypress")
    registered_keypress.text = data.registered_keypress

    transaction_id = ET.Element("transactionId")
    transaction_id.text = flag_char

    root.append(registered_keypress)
    root.append(timestamp)
    root.append(status)
    root.append(transaction_id)

    return root


def server_loop():
    flag_indexes = list(range(1, len(FLAG) + 1))
    random.shuffle(flag_indexes)
    logging.info("flag_indexes: %s", flag_indexes)

    counter = 0
    sent = 0
    while data := read_till_0():
        metric_data = get_metric(data)
        logging.info(metric_data)
        counter += 1

        if counter == sum(flag_indexes[: sent + 1]):
            index_to_send = flag_indexes[sent] - 1
            char_to_send = FLAG[index_to_send]
            logging.info("sending flag char `%s` index `%s`", char_to_send, index_to_send)
            xml_res = response_flag_char(metric_data, char_to_send)
            sent += 1
        else:
            xml_res = response_filling_buffer(metric_data)

        # reset counter and sent once flag was correctly sent
        if sent == len(flag_indexes):
            counter = 0
            sent = 0

        # send response
        print(ET.tostring(xml_res).decode(), end="\0", flush=True)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)

    with open("/flag.txt") as f:
        FLAG = f.read().strip()
    assert FLAG

    server_loop()
