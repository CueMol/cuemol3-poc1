import * as React from 'react';
import ReactDOM from 'react-dom';
import './index.css';

import { App } from './App.jsx';
import { MolViewProvider } from './hooks/useMolView.jsx';
import { CueMolProvider } from './hooks/useCueMol.jsx';

ReactDOM.render(
  <CueMolProvider>
    <MolViewProvider>
      <App />
    </MolViewProvider>
  </CueMolProvider>,
  document.querySelector("#root")
);
