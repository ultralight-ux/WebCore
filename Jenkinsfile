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
               echo $PATH
               which make
               ./make
            '''
          }
        }
      }
    }
  }
}