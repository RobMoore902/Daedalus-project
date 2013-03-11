if ( Drupal.jsEnabled ) {
$(document).ready(function() {

  /**
    * send a http request to remove an item from a list.
    * Requirements:
    *   - An anchor tag in the list item with the class removable.
    *   - An href with the link to the http request to be made.
    *   - Anchor tag should also have an attribute "message" which
    *     contains a message for the javascript confirmation message.
    */
  $(".removable").each(function() {
    $(this).click(function(event) {
      // Prevent link (href) from
      // being followed through.
      event.preventDefault();
      message = $(this).attr("message");
      if(confirm(message)) {
        $(this).parent().fadeOut(3500);
        $.get($(this).attr("href"));
      }
    });
  });

  /**
   * Show helping information.
   */
  $(".show-slo-help").mousedown(function(){
    Drupal.checkPlain(alert("Adding Student Learing Outcomes:" + 
      "\n\t-Search for an existing learning outcome or..." + 
      "\n\t-Use the character '#' to search by labels/tags." + 
      "\n\t-When a single label/tag is narrowed down:" + 
      "\n\t\t-Enter the #lable/#tag to enter all associated outcomes." + 
      "\n\t\t-Select a single learning outcome."));
  });

  $(".show-pcourse-help").mousedown(function(){
    Drupal.checkPlain(alert("Adding Prerequisite Courses:" +
      "\n\t-Search for an existing course in the curriculum." + 
      "\n\t-Create a non-course text prerequisite." + 
      "\n\t-Use 'OR' to search and add a prerequisite set." + 
      "\n\t\t-Only courses within the curriculum may be added to a prerequisite set."));
  });

  /*
   * Hide all class 'hide-prereq-course' divs where
   * the attribute 'is_hidden' is set to yes.
   */
  $(".hide-prereq-course").each(function() {
    if( $(this).attr("is_hidden") == "yes" ) {
      $(this).hide();
    }
  });

  /*
   * The add another button with class 'add-prereq-course' will search the
   * DOM for a div with class 'hide-prereq-course'. When an element is found
   * a flag is set to prevent searching for another and the element is shown.
   */
  $(".add-prereq-course").click(function(event) {
    event.preventDefault();
    var found = false;
    $(".hide-prereq-course").each(function() {
      if ( found == false && $(this).attr("is_hidden") == "yes" ) {
        found = true;
        $(this).attr("is_hidden","no");
        $(this).show("slow");
      }
    });
    found = false;
    $(".hide-prereq-course").each(function() {
      if ( found == false && $(this).attr("is_hidden") == "yes" ) {
        found = true;
      }
    });
    if( found == false ) {
      $(this).hide(4000);
      alert("The maximum amount of additional prerequisite courses has been entered.");
    }
  });

  /**
   * Hide all class 'hide-prereq-course' divs where
   * the attribute 'is_hidden' is set to yes.
   */
  $(".hide-slo").each(function() {
    if( $(this).attr("is_hidden") == "yes" ) {
      $(this).hide();
    }
  });

  /*
   * The add another button with class 'add-slo' will search the
   * DOM for a div with class 'hide-slo'. When an element is found a
   * flag is set to prevent searching for another and the element is shown.
   */
  $(".add-slo").click(function(event) {
    event.preventDefault();
    var found = false;
    $(".hide-slo").each(function() {
      if ( found == false && $(this).attr("is_hidden") == "yes" ) {
        found = true;
        $(this).attr("is_hidden","no");
        $(this).show("slow");
      }
    });
    found = false;
    $(".hide-slo").each(function() {
      if ( found == false && $(this).attr("is_hidden") == "yes" ) {
        found = true;
      }
    });
    if( found == false ) {
      $(this).hide(4000);
      alert("The maximum amount of additional student learning outcomes has been entered.");
    }
  });

});
}