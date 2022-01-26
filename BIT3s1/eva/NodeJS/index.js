/*
npm init -y
npm install express
node .
*/



const express = require('express');
const app = express();

app.get('/', (request, response) => {
    
   response.status(200).send('Hello World VUT FIT!');
   
});

app.get('/lol', (request, response) => {
    
   response.status(200).send('NodeJS!');
   
});

app.listen(process.env.PORT || 3000, () => console.log('App is available on: http://eva.fit.vutbr.cz:3000'));

