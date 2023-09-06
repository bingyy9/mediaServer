'use strict'

const http = require('http');
const express = require('express');
const app = express();

const http_server = http.createServer(app);
http_server.listen(9999, '0.0.0.0');   //0.0.0.0 监听本机任意网卡