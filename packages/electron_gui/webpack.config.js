// import path from 'path';
const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
// import { Configuration } from 'webpack';

const mainConfig = {
  // mode: "production",
  mode: "development",
  entry: "./src/electron.ts",
  target: "electron-main",
  module: {
    rules: [
      {
        test: /\.ts$/,
        include: /src/,
        resolve: {
          extensions: [".ts", ".js"],
        },
        use: [{ loader: "ts-loader" }],
      },
    ],
  },
  output: {
    devtoolModuleFilenameTemplate: "[absolute-resource-path]",
    path: __dirname + "/dist",
    filename: "electron.js",
  },
  node: {
    __dirname: false,
  },
  // plugins: [new HardSourceWebpackPlugin()],
};

const rendererConfig = {
  mode: 'development',
  entry: './src/index.jsx',
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
        options: {
          presets: [
            '@babel/preset-env',
            '@babel/preset-react'
          ],
          plugins: ['@babel/plugin-transform-runtime'],
        },
      },
      {
        test: /style.css$/,
        use: ["style-loader", "css-loader"],
      },
      {
        test: /react-tabs.css$/,
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

const workerConfig = {
  mode: 'development',
  entry: './src/worker/main.ts',

  module: {
    rules: [
      {
        test: /\.ts$/,
        include: /src/,
        resolve: {
          extensions: [".ts", ".js"],
        },
        use: [{ loader: "ts-loader" }],
      },
    ],
  },

  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: 'worker.js',
    libraryTarget: 'commonjs2',
  },
  externals: [
    '@cuemol/core',
  ],
  devtool: 'inline-source-map',
};

const preloadConfig = {
  mode: 'development',
  entry: './src/preload.ts',
  target: 'electron-preload',
  module: {
    rules: [
      {
        test: /\.ts$/,
        include: /src/,
        resolve: {
          extensions: [".ts", ".js"],
        },
        use: [{ loader: "ts-loader" }],
      },
    ],
  },
  output: {
    path: __dirname + "/dist",
    filename: "preload.js",
    libraryTarget: 'commonjs2',
  },
  externals: [
    'electron',
  ],
  devtool: 'inline-source-map',
};

module.exports = [mainConfig, rendererConfig, workerConfig, preloadConfig];
