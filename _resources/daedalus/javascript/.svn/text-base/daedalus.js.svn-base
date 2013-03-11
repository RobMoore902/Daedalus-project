if ( Drupal.jsEnabled ) {
$(document).ready(function() {

  /*
   * hide divs with the id "#content-top"
   */
  $(function(){
    var hideStatus = function() {
      $("#content-top").slideUp(3000);
    }
    setTimeout(hideStatus, 45000);
  });

  /*
   * Hides all items of class 'hide-help'
   */
  $(".hide-help").each(function() {
    //hide all the ones marked as hidden
    if( $(this).attr("is_hidden") == "yes" ) {
      $(this).hide();
    }
  });

  /*
   * When the button with class 'show-help' is clicked, all items of class 
   * 'hide-help' have the attribute "is_hidden" set to no if already set to
   * yes, and vise versa. Also the button has the class permanent-opaque
   * added and removed. The use of two different classes makes it easy to
   * differentiate the click function with a mouse over.
   */
  $(".show-help").click(function() {
    if ( $(this).hasClass("permanent-opaque") ) {
      $(this).removeClass("permanent-opaque");
    }
    else {
      $(this).addClass("permanent-opaque");
    }
    $(".hide-help").each(function() {
      // Change the element to not hidden and show it.
      if ( $(this).attr("is_hidden") == "yes" ) {
        $(this).attr("is_hidden","no");
        $(this).show("slow");
      }
      // Change the element so it is hidden and hide it.
      else if ( $(this).attr("is_hidden") == "no" ) {
          $(this).attr("is_hidden","yes");
          $(this).hide("slow");
      }
    });
  });

  /*
   * This provies the mouse over 
   * to opaque the given element.
   */
  $(".show-help").each(function() {
    $(this).mouseover(function(){
      $(this).addClass("opaque");
    });
    $(this).mouseout(function(){
      $(this).removeClass("opaque");
    });
  });
});
}