/**
 * @file
 * Sets up behaviors for the site register form
 */

(function ($) {
  var purls = {},
    timers = {
      name: 0,
      email: 0
    };

  Drupal.behaviors.ossapRegister = {
    attach: function (ctx) {

      $('#edit-site-type').change(changeSiteType).change();
      $('#edit-preset').change(changePreset).change();
      $('#edit-purl').keyup(checkPurl);

      $('#domain').replaceWith($('#edit-domain'));
      $('#edit-domain')
        .change(checkPurl)
        .change(nameValidate)
        .change(emailValidate);
      $('.form-item-domain').remove();

      $('#edit-name').keyup(queueNameValidation);
      $('#edit-email').keyup(queueEmailValidation);

      var servers = Drupal.settings.ossap.servers;
      for (var i in servers) {
        var xhr = new XMLHttpRequest();

        xhr.open('GET', 'http://'+i+'/site/purls');
        xhr.onreadystatechange = function (xhr) {
          xhr = xhr.target;
          if (xhr.readyState == 4 && xhr.status < 400) {
            var data = JSON.parse(xhr.response);
            purls[data.domain] = data.purls;
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

  function queueNameValidation() {
    if (timers.name) {
      clearTimeout(timers.name);
    }

    timers.name = setTimeout(nameValidate, 500);
  }

  function queueEmailValidation() {
    if (timers.email) {
      clearTimeout(timers.email);
    }

    timers.email = setTimeout(emailValidate, 500);
  }

  function nameValidate() {
    var name = $('#edit-name').val(),
      domain = $('#edit-domain').val();

    if (name && domain) {
      var xhr = new XMLHttpRequest();

      xhr.open('GET', 'http://'+domain+'/site/register/validate/name/'+name);
      xhr.onreadystatechange = function (xhr) {
        xhr = xhr.target;
        if (xhr.readyState == 4 && xhr.status < 400) {
          var data = JSON.parse(xhr.responseText);
          if (!data.valid) {
            $('#name-errors').html(data.message);
          }
          else {
            $('#name-errors').html('This username is available.');
          }
        }
      };
      xhr.send();
    }
  }

  function emailValidate() {
    var email = $('#edit-email').val(),
      domain = $('#edit-domain').val();

    if (email && domain) {
      var xhr = new XMLHttpRequest();

      xhr.open('GET', 'http://'+domain+'/site/register/validate/email/'+email);
      xhr.onreadystatechange = function (xhr) {
        xhr = xhr.target;
        if (xhr.readyState == 4 && xhr.status < 400) {
          var data = JSON.parse(xhr.responseText);
          if (!data.valid) {
            $('#email-errors').html(data.message);
          }
          else {
            $('#email-errors').html('This email is available.');
          }
        }
      };
      xhr.send();
    }
  }

})(jQuery);
