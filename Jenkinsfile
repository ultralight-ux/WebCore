pipeline {
  agent none
  stages {
    stage('Build') {
      parallel {
        stage('Build macOS') {
          agent {
            node {
              label 'mac_x64'
            }

          }
          steps {
            sh '''
               PATH=$PATH:/usr/local/bin
               echo $PATH
               which cmake
               ./make
            '''
          }
        }
      }
    }
  }
}