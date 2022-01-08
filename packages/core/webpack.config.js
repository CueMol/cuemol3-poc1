// import path from 'path';
const path = require('path');
// import { Configuration } from 'webpack';

module.exports = {
  mode: 'development',
  entry: ['./index.js'],
  output: {
    path: path.resolve(__dirname, 'lib'),
    filename: 'index.js',
    library: {
      type: 'commonjs'
    }
  },
  externals: [
//    bindings: 'bindings',
//    path: 'path',
    function({ request }, callback) {
      if (request === 'bindings' || request === 'path') {
        return callback(null, 'commonjs ' + request);
      }
      callback();
    }
  ],
  devtool: 'inline-source-map',
};

// export default config;
