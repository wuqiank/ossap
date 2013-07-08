/**
 * @file
 * Sets up behaviors for the site register form
 */

(function ($) {
  var purls = {};

  Drupal.behaviors.ossapRegister = {
    attach: function (ctx) {

      $('#edit-site-type').change(changeSiteType).change();
      $('#edit-preset').change(changePreset).change();
      $('#edit-purl').keyup(checkPurl);

      $('#domain').replaceWith($('#edit-domain'));
      $('#edit-domain').change(checkPurl);
      $('.form-item-domain').remove();

      var servers = Drupal.settings.ossap.servers;
      for (var i in servers) {
        var xhr = new XMLHttpRequest();

        xhr.open('GET', 'http://'+i+'/site/purls');
        xhr.onreadystatechange = function (xhr) {
          xhr = xhr.target;
          if (xhr.readyState == 4 && xhr.status < 400) {
            data = JSON.parse(xhr.response);
            purls[data.domain] = data.purls;
            console.log(purls);
          }
        };
        xhr.send();
      }
    }
  };

  function changeSiteType() {
    var val = $(this).val(),
      presets = (val != "")?Drupal.settings.ossap.preset[val]:[],
      servers = Drupal.settings.ossap.servers,
      domains = [];

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
      for (var i in servers) {
        if ($.inArray(val, servers[i]['types']) != -1) {
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
    checkPurl();
  }

  function changePreset() {
    var preset = $(this).val();

    if (preset == '') {
      $('.form-item-purl').hide();
    }
    else {
      $('.form-item-purl').show();
    }
  }

  function checkPurl() {
    var val = $('#edit-purl').val(),
        domain = $('#edit-domain').val(),
        servers = Drupal.settings.ossap.servers,
        check = [];

    for (var i in servers) {
      if ($.inArray(domain, servers[i]['domains']) != -1 && 'http://'+i in purls) {
        check = purls['http://'+i];
      }
    }

    if (check.length == 0) {
      $('#edit-purl + .error').hide();
      enableSubmit(false);
    }
    else if ($.inArray(val, check) == -1) {
      $('#edit-purl + .error').hide();
      enableSubmit(true);
    }
    else {
      if ($('#edit-purl + .error').length == 0) {
        $('#edit-purl').after('<div class="error">This site name has been taken. Please enter another.</div>');
      }
      $('#edit-purl + .error').show();
      enableSubmit(false);
    }
  }

  function enableSubmit(enable) {
    if (enable) {
      $('#edit-create').attr('disabled', '');
    }
    else {
      $('#edit-create').attr('disabled', 'disabled');
    }
  }

})(jQuery);
