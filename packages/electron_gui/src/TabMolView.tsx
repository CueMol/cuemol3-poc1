import React, { useEffect, useRef } from 'react';
import { MolView } from './MolView';
import { Tab, Tabs, TabList, TabPanel } from 'react-tabs';
import 'react-tabs/style/react-tabs.css';
import styles from './TabMolView.css';
import { useMolView } from './hooks/useMolView';

// <button className={styles.tabs}>xxx</button>

export const TabMolView: React.FC = () => {

  const { molViewTabs, setActiveTab } = useMolView();
  const onSelectFn = (ind, prev_ind, ...args) => {
    console.log('on select called!!!', args);
    setActiveTab(ind);
  };

  console.log('molViewTabs.length:', molViewTabs.length);
  const tabs = molViewTabs.map( tab => {
    return (
      <Tab>{ tab.title }:{ tab.active?1:0 }</Tab>
    );
  });

  return (
    <div className={styles.tabMolView}>
      <Tabs className={styles.tabs} onSelect={onSelectFn}>
        <TabList className={styles.tabList}>
          { tabs }
          <Tab disabled>+</Tab>
        </TabList>
      </Tabs>
      <MolView className={styles.panel}/>
    </div>
  );
}
