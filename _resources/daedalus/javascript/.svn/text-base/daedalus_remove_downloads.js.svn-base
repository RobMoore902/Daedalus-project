if ( Drupal.jsEnabled ) {
$(document).ready(function() {
  /**
   * Give the browser 1 second to open the file location.
   * Then the attributes href will delete the files directory.
   */
  $(".remove-download").mousedown(function(){
    var removeDownload = function() {
      $.get($(this).attr("remove_url"));
    }
    setTimeout(removeDownload, 1000);
  });
});
}