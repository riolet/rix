node('gcc') {
  stage("Checkout") {
    git url: "https://github.com/riolet/rix", branch: "master"
  }
  stage("Build") {
    sh "make"
  }
  stage("Run tests"){
    sh "cd tests && chmod +x runtests.sh && ./runtests.sh"
  }
}
