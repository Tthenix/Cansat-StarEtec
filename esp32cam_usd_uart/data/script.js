document.addEventListener("DOMContentLoaded", function() {
    const cameraStream = document.getElementById("camera-stream");
    const captureButton = document.getElementById("capture-button");
  
    const updateCameraStream = () => {
      cameraStream.src = "/image?" + new Date().getTime();
    };
  
    const captureImage = () => {
      fetch("/capture")
        .then(response => response.text())
        .then(message => {
          alert(message);
          updateCameraStream();
        });
    };
  
    captureButton.addEventListener("click", captureImage);
    updateCameraStream();
  });
  