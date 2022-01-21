// import path from 'path';
const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
// import { Configuration } from 'webpack';

module.exports = {
  mode: 'development',
  // entry: ['./src/index.js'],
  entry: ['./src/index.jsx'],
  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: 'index.js',
  },
  devtool: 'inline-source-map',
  plugins: [
    new HtmlWebpackPlugin({
      template: "./src/index.html"
    })
  ],
  module: {
    rules: [
      {
        test: /\.jsx$/,
        exclude: /(node_modules)/,
        loader: "babel-loader",
      },
      {
        test: /style.css$/,
        use: ["style-loader", "css-loader"],
      },
      {
        test: /\.css$/,
        exclude: /node_modules/,
        use: [{loader: 'style-loader'},
              {loader: 'css-loader', options: { modules: true }}],
      }
    ]
  }
};

// export default config;
