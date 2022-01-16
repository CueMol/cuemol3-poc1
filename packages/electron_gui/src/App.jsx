import * as React from 'react';
import ReactDOM from 'react-dom';
import styles from './App.css';
import { SidePanel } from './SidePanel.jsx';
import { MolView } from './MolView.jsx';

export function App() {
  return (
    <div className={styles.content}>
      <div className={styles.menuContainer}>Menu</div>
      <div className={styles.appContainer}>

        <div className={styles.sidePanelContainer}>
          <SidePanel />
          <SidePanel />
          <SidePanel />
        </div>
        <div id="placeholder" className={styles.mainContainer}>
          <MolView />
          <div className={styles.bottomContainer}>Bottom Panel</div>
        </div>

      </div>
      <div className={styles.statusContainer}>Status</div>

    </div>
  );
}
