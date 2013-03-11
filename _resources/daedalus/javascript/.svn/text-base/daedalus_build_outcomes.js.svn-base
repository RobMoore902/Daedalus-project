if ( Drupal.jsEnabled ) {
$(document).ready(function() {

  /*
   * Hide all class 'hide-covering-course' divs where
   * the attribute 'is_hidden' is set to yes.
   */
 $(".hide-covering-course").each(function() {
    if( $(this).attr("is_hidden") == "yes" ) {
      $(this).hide();
    }
  });

  /*
   * The add another button with class 'add-covering-course' will search the
   * DOM for a div with class 'hide-covering-course'. When an element is found
   * a flag is set to prevent searching for another and the element is shown.
   */
  $(".add-covering-course").click(function(event) {
    event.preventDefault();
    var found = false;
    $(".hide-covering-course").each(function() {
      if ( found == false && $(this).attr("is_hidden") == "yes" ) {
        found = true;
        $(this).attr("is_hidden","no");
        $(this).show("slow");
      }
    });
    found = false;
    $(".hide-covering-course").each(function() {
      if ( found == false && $(this).attr("is_hidden") == "yes" ) {
        found = true;
      }
    });
    if( found == false ) {
      $(this).hide(4000);
      alert("The maximum amount of additional covering courses has been entered.");
    }
  });

  /*
   * Hide all class 'hide-prereq-slo' divs where
   * the attribute 'is_hidden' is set to yes.
   */
 $(".hide-prereq-slo").each(function() {
    if( $(this).attr("is_hidden") == "yes" ) {
      $(this).hide();
    }
  });

  /*
   * The add another button with class 'add-prereq-slo' will search the
   * DOM for a div with class 'hide-prereq-slo'. When an element is found
   * a flag is set to prevent searching for another and the element is shown.
   */
  $(".add-prereq-slo").click(function(event) {
      event.preventDefault();
      var found = false;
      $(".hide-prereq-slo").each(function() {
        if ( found == false && $(this).attr("is_hidden") == "yes" ) {
          found = true;
          $(this).attr("is_hidden","no");
          $(this).show("slow");
        }
      });
      found = false;
      $(".hide-prereq-slo").each(function() {
        if ( found == false && $(this).attr("is_hidden") == "yes" ) {
          found = true;
        }
      });
      if( found == false ) {
        $(this).hide(4000);
        alert("The maximum amount of additional prerequisite learning outcomes has been entered.");
      }
  });

  /**
   * The following functions show information when 
   * adding information to a given course or SLO.
   */
  $(".show-tag-help").mousedown(function(){
    Drupal.checkPlain(alert("Adding Tags:" + 
      "\n\t-To enter multiple tags, seperate each tag with a comma" + 
      "\n\t-To view the list by a given character enter '*a-z'" + 
      "\n\t-To view the complete list of tags enter '*'"));
  });

  $(".show-preslo-help").mousedown(function(){
    Drupal.checkPlain(alert("Adding Prerequiste Learing Outcomes:" + 
      "\n\t-Search for an existing learning outcome or..." + 
      "\n\t-Use the character '#' to search by labels/tags." + 
      "\n\t-When a single label/tag is narrowed down:" + 
      "\n\t\t-Enter the #lable/#tag to enter all associated outcomes." + 
      "\n\t\t-Select a single learning outcome."));
  });

});
}