import * as React from 'react';
import { createRoot } from 'react-dom/client';
import './index.css';

import { App } from './App';
import { MolViewProvider } from './hooks/useMolView';
import { CueMolProvider } from './hooks/useCueMol';

const root = createRoot(document.getElementById('root')!);
root.render(
  <CueMolProvider>
    <MolViewProvider>
      <App />
    </MolViewProvider>
  </CueMolProvider>
);
