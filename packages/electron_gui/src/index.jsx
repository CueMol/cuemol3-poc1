import * as React from 'react';
import ReactDOM from 'react-dom';
import './index.css';
import { App } from './App.jsx';
import { MolViewProvider } from './use_molview.jsx';

ReactDOM.render(
    <MolViewProvider>
    <App />
    </MolViewProvider>,
  document.querySelector("#root")
);
