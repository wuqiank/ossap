/**
 * @file
 * Sets up behaviors for the site register form
 */

(function ($) {
  Drupal.behaviors.ossapRegister = {
    attach: function (ctx) {

      $('#edit-site-type').change(changeSiteType).change();
      $('#ossap-register-site-register-form').submit(submit);
    }
  };

  function changeSiteType() {
    var $this = $(this),
      val = $this.val(),
      presets = (val != "")?Drupal.settings.ossap.preset[val]:[];

    $('#edit-preset').val('').find('option').each(function (i) {
      if (this.value == "" || $.inArray(this.value, presets) != -1) {
        $(this).show();
      }
      else {
        $(this).hide();
      }
    });
  }

  function submit(e) {
    // TODO: Use correct domain instead of basePath
    e.preventDefault();

    $('#edit-create').attr('disabled', 'disabled').after('<div class="ajax-progress ajax-progress-throbber"><div class="throbber">&nbsp;</div></div>');

    var domain = Drupal.settings.basePath,
        url = domain+'vsite.json';

    try {
      var xhr = new XMLHttpRequest();
      xhr.open('POST', url);
      xhr.setRequestHeader('Content-Type', 'application/json');
      xhr.onreadystatechange = function (e) {
        $('.ajax-progress').remove();
        $('#edit-create').replaceWith('<div class="success"><a href="'+domain+$('#edit-purl').val()+'">Your new site has been created! Click here to go to it!</a></div>')
      };
      xhr.send(JSON.stringify({
        title: $('#edit-purl').val(),
        type: $('#edit-site-type').val(),
        preset: $('#edit-preset').val(),
        domain: $('#edit-purl').val(), // not needed for now. domain is auto-determined
        purl: $('#edit-purl').val(),
        visibility: $('input:radio[name=vsite_private]:checked').val(),
        owner: -1
      }));
    }
    catch (e) {
      console.log(e);
    }
  }
})(jQuery);
