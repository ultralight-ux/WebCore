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
            sh './make'
          }
        }
        stage('Build Windows x64') {
          agent {
            node {
              label 'win_x64'
            }

          }
          steps {
            sh 'make release x64'
          }
        }
        stage('Build Windows x86') {
          agent {
            node {
              label 'win_x86'
            }

          }
          steps {
            sh 'make release x86'
          }
        }
        stage('Build Linux x64') {
          agent {
            node {
              label 'linux_x64'
            }

          }
          steps {
            sh './make'
          }
        }
      }
    }
  }
}