import React, { useEffect } from 'react';
import { MolView } from './MolView.jsx';
import { Tab, Tabs, TabList, TabPanel } from 'react-tabs';
import 'react-tabs/style/react-tabs.css';
import styles from './TabMolView.css';

// <button className={styles.tabs}>xxx</button>

export function TabMolView() {
  const onSelectFn = () => {
    console.log('on select called!!!');
  };
  return (
    <div className={styles.tabMolView}>
      <Tabs className={styles.tabs} onSelect={onSelectFn}>
        <TabList className={styles.tabList}>
          <Tab>Tab 1</Tab>
          <Tab>Tab 2</Tab>
          <Tab>Tab 3</Tab>
          <Tab>Tab 4</Tab>
        </TabList>
      </Tabs>
      <MolView className={styles.panel} />
    </div>
  );
};
