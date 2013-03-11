if ( Drupal.jsEnabled ) {
$(document).ready(function() {
  $(".show-tag-help").mousedown(function(){
    Drupal.checkPlain(alert("Adding Tags:" + 
      "\n\t-To enter multiple tags, seperate each tag with a comma" + 
      "\n\t-To view the list by a given character enter '*a-z'" + 
      "\n\t-To view the complete list of tags enter '*'"));
  });
});
}