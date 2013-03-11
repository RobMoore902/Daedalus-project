$(document).ready(function() {

  /**
   * hide a session timeout warning from an advisor
   * for a period of pre-determined milliseconds.
   */
  $(function(){
    // Get the millisecs attribute which is the amount of milli-
    // seconds before or after the 10 minute session times out.
    var millisecs = parseInt($("div.timeout").attr("millisecs"));
    var i;
    // A function to remove a minute from the countdown display.
    var removeMinute = function() {
      var minuteVal = parseInt($("a.minutes").html());
      minuteVal = minuteVal - 1;
      $("a.minutes").text(minuteVal);
    }
    // A function to reload the current page.
    var timeoutUser = function() {
      location.reload(true);
    }
    // If the millisecond value is greater than zero there session
    // is more than 10 minutes away from timing out. The millisecs
    // value indicates how many milliseconds remain.
    if ( millisecs >= 0 ) {
      // Hide the warning
      $("div.timeout").hide();
      // Create a function to show the warning, and 
      // set a timeout to show it when the session 
      // reaches exactly 10 minutes remaining.
      var showTimeout = function() {
        $("div.timeout").fadeIn(2500);
      }
      setTimeout(showTimeout, millisecs);
      // Start the session countdown
      for(i=1;i<10;i++) {
        setTimeout( removeMinute, (millisecs + (i*60000) ) );
      }
      // Reload the page if the countdown is reached without
      // the user adding more time to the session. The session
      // will have timed out and the logout script will run.
      setTimeout( timeoutUser, (millisecs + (10*60000) ) );
    }

    // If the millisecond value is less than zero the session
    // is less than 10 minutes from timing out. Do not hide the
    // session timeout warning and begin the countdown.
    if ( millisecs < 0 ) {
      millisecs = millisecs + 600000;
      var minutes = Math.floor((millisecs / 1000) / 60);
      var seconds = Math.floor((millisecs / 1000) % 60);

      setTimeout( removeMinute, (seconds*1000));

      for(i=1;i<minutes;i++) {
        setTimeout( removeMinute, ((seconds*1000) + (i*60000) ) );
      }
      // Reload the page if the countdown is reached without
      // the user adding more time to the session. The session
      // will have timed out and the logout script will run.
      setTimeout( timeoutUser, ((seconds*1000) + (minutes*60000) ) );
    }
  });
});