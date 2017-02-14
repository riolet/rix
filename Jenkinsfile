node('gcc') {
  stage("Checkout") {
    checkout scm 
  }
  stage("Build") {
    sh "make"
  }
  stage("Run tests"){
    sh "cd tests && chmod +x runtests.sh && ./runtests.sh"
  }
}
