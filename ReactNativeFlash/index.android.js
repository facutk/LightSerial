/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 * @flow
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  View,
  Button
} from 'react-native';
import RNFlash from 'react-native-flash';
import morseMessageTiming from './morseMessageTiming';

RNFlash.turnOnFlash(); // turn on flash
		
RNFlash.turnOffFlash(); // turn off flash
		
RNFlash.hasFlash(function(){
  //RNFlash.turnOnFlash();
},function(){
  alert("You do not have flash")
});

export default class AwesomeProject extends Component {
  constructor(props, context) {
    super(props, context);
    this.state = {
      flash: false,
      profile: 'Profile'
    };
  }

  handleFlashToggle = () => {
    this.setState({
      flash: !this.state.flash
    })

    if (this.state.flash) {
      RNFlash.turnOnFlash();
    } else {
      RNFlash.turnOffFlash();
    }
  }

  handleProfile = () => {
    this.setState({
      profile: 'Profiling...'
    })

    const start_time = new Date();
    const max_loops = 100;
    for(var i = 0; i < max_loops; i++) {
      RNFlash.turnOnFlash();
      RNFlash.turnOffFlash();
    }
    const end_time = new Date();
    const profile_time = end_time - start_time;

    this.setState({
      profile: `${max_loops} loops took ${profile_time}ms`
    })
  }

  handleMorse = () => {
    morseMessageTiming('facutk', 100, (action) => {
      switch (action) {
        case "ON":
          RNFlash.turnOnFlash();
          break;
        case "OFF":
          RNFlash.turnOffFlash();
          break;
      }
    });
  }

  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>
          Elementum
        </Text>
        <Text style={styles.instructions}>
          react-native
        </Text>
        <Button
          onPress={this.handleFlashToggle}
          title="Toggle Flash"
          color="#841584"
        />
        <Button
          onPress={this.handleProfile}
          title={this.state.profile}
          color="#841584"
        />
        <Button
          onPress={this.handleMorse}
          title="morse: facutk"
          color="#99cc00"
        />
        <Text style={styles.instructions}>
          Double tap R on your keyboard to reload,{'\n'}
          Shake or press menu button for dev menu
        </Text>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#F5FCFF',
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
  instructions: {
    textAlign: 'center',
    color: '#333333',
    marginBottom: 5,
  },
});

AppRegistry.registerComponent('AwesomeProject', () => AwesomeProject);
