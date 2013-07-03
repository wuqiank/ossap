/**
 * @file
 * Sets up behaviors for the site register form
 */

(function ($) {
  var purls = [];

  Drupal.behaviors.ossapRegister = {
    attach: function (ctx) {

      $('#edit-site-type').change(changeSiteType).change();
      $('#edit-preset').change(changePreset).change();
      $('#edit-purl').change(changePurl);
      $('#ossap-register-site-register-form').submit(submit);

      $('#domain').replaceWith($('#edit-domain'));
      $('.form-item-domain').remove();

      var servers = Drupal.settings.ossap.servers;
      for (var i in servers) {
        var xhr = new XMLHttpRequest();

        xhr.open('GET', 'http://'+i+'/site/purls');
        xhr.setRequestHeader('Origin', location.origin);
        xhr.onreadystatechange = recievePurls;
        xhr.send();
      }
    }
  };

  function recievePurls (xhr) {
    console.log(xhr);
  }

  function changeSiteType() {
    var val = $(this).val(),
      presets = (val != "")?Drupal.settings.ossap.preset[val]:[];

    if (val == '') {
       $('.form-item-preset').hide();
    }
    else {
      $('.form-item-preset').show();
      $('#edit-preset').val('').find('option').each(function (i) {
        if (this.value == "" || $.inArray(this.value, presets) != -1) {
          $(this).show();
        }
        else {
          $(this).hide();
        }
      });
    }
  }

  function changePreset() {
    var preset = $(this).val(),
        servers = Drupal.settings.ossap.servers,
        domains = [];

    if (preset == '') {
      $('.form-item-purl').hide();
    }
    else {
      $('.form-item-purl').show();
      for (var i in servers) {
        if ($.inArray(preset, servers[i]['presets']) != -1) {
          domains = domains.concat(servers[i]['domains']);
        }
      }

      $('#edit-domain').find('option').each(function (i) {
        if (this.value == "" || $.inArray(this.value, domains) != -1) {
          $(this).show();
        }
        else {
          $(this).hide();
        }
      });
    }
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
