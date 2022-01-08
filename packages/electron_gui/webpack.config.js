// import path from 'path';
const path = require('path');
// import { Configuration } from 'webpack';

module.exports = {
  mode: 'development',
  entry: ['./src/index.js'],
  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: 'index.js',
  },
  devtool: 'inline-source-map',
};

// export default config;
