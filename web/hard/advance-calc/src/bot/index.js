// create express app which, in post request, start puppeteer and go to CHALLENGE_URL

const express = require('express');
const puppeteer = require('puppeteer');


const CHALLENGE_URL = 'http://localhost:1337';
const FLAG = process.env.FLAG || 'wpctf{fake_flag}';

const app = express();

app.use(express.json());

const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

app.post('/bot', async (req, res) => {

    const browser = await puppeteer.launch({
        headless: true,
        executablePath: '/usr/bin/chromium',
        args: ['--no-sandbox', '--disable-setuid-sandbox']
    });
    try{
        const page = await browser.newPage();
    
        await page.goto(CHALLENGE_URL);
        console.log('Page loaded');
    
        // add cookie
        await page.setCookie({
            name: 'flag',
            value: FLAG,
            domain: 'localhost',
        });
        console.log('Cookie added');

        // type the expression
        // delete input value 
        await page.evaluate(() => {
            document.querySelector('input[name=expression]').value = '';
        });
        
        await page.type('input[name=expression]', '15+18');
        await page.click('input[name=_submit]');
        console.log('Expression typed');
    
        // wait for the result
        console.log('Waiting for 3 seconds');
        await sleep(3000);
        console.log('Waited for 3 seconds');
    
    } catch (e) {
        console.error(e);
    }

    console.log('Closing browser');
    await browser.close();

    res.send('Bot finished');
   
});

app.listen(3000, () => {
    console.log('Bot listening on port 3001');
}
);
