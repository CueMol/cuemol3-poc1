import * as React from 'react';
import ReactDOM from 'react-dom';
import './index.css';

import { App } from './App';
import { MolViewProvider } from './hooks/useMolView';
import { CueMolProvider } from './hooks/useCueMol';

ReactDOM.render(
  <CueMolProvider>
    <MolViewProvider>
      <App />
    </MolViewProvider>
  </CueMolProvider>,
  document.querySelector("#root")
);
