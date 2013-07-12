/**
 * @file
 * Sets up behaviors for the site register form
 */

(function ($) {
  var purls = {},
    timers = {
      name: 0,
      email: 0
    },
    preset_options = [],
    domain_options = [];

  Drupal.behaviors.ossapRegister = {
    attach: function (ctx) {

      if ($(ctx).find('form').length > 0) {

        $('#edit-site-type').change(changeSiteType).change();
        $('#edit-preset').change(changePreset).change();
        $('#edit-purl').keyup(checkPurl);

        $('#domain').replaceWith($('#edit-domain'));
        $('#edit-domain')
          .change(checkPurl)
          .change(userCheck)
          .change(nameValidate)
          .change(emailValidate);
        $('.form-item-domain').remove();

        $('#edit-name').keyup(queueNameValidation);
        $('#edit-mail').keyup(queueEmailValidation);

        $('#user-tabs').tabs().hide();

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
        enableSubmit(false);
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
          $(this).attr('disabled', '');
        }
        else {
          $(this).attr('disabled', 'disabled');
        }
      });
      for (var i in servers) {
        if ('http://'+i in purls && $.inArray(val, servers[i]['types']) != -1) {
          domains = domains.concat(servers[i]['domains']);
        }
      }

      $('#edit-domain').find('option').each(function (i) {
        if (this.value == "" || $.inArray(this.value, domains) != -1) {
          $(this).attr('disabled', '');
        }
        else {
          $(this).attr('disabled', 'disabled');
        }
      });
      if (domains.length == 0) {
        enableSubmit(false);
      }
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
    $('#submission-errors').remove();
    if (enable) {
      $('#edit-submit').attr('disabled', '');
    }
    else {
      $('#edit-submit').attr('disabled', 'disabled').before('<div id="submission-errors">Some of what you have given us is invalid. Please correct it before continuing.</div>');
    }
  }

  function userCheck() {
    var domain = $('#edit-domain').val(),
      servers = Drupal.settings.ossap.servers,
      pins = Drupal.settings.ossap.pins;

    if (typeof pins == 'undefined') {
      $('#user-tabs').show();
    }
    else {
      for (var i in servers) {
        if ($.inArray(domain, servers[i]['domains']) != -1 && 'http://'+i in purls) {
          $('#pin-user').val(pins[i]);
          if (pins[i]) {
            $('#user-tabs').hide();
          }
          else {
            $('#user-tabs').show();
            $('#target-domain').html('http://'+domain);
          }
        }
      }
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
          try {
            var data = JSON.parse(xhr.responseText);
            if (!data.valid) {
              $('#name-errors').html(data.message.join('<br />'));
              enableSubmit(false);
            }
            else {
              $('#name-errors').html('This username is available.');
              enableSubmit(true);
            }
          }
          catch (e) {
            console.log(e);
          }
        }
      };
      xhr.send();
      $('#name-errors').html('<div class="ajax-progress ajax-progress-throbber"><div class="throbber">&nbsp;</div></div>');
    }
  }

  function emailValidate() {
    var email = $('#edit-mail').val(),
      domain = $('#edit-domain').val();

    if (email && domain) {
      var xhr = new XMLHttpRequest();

      xhr.open('GET', 'http://'+domain+'/site/register/validate/mail/'+email);
      xhr.onreadystatechange = function (xhr) {
        xhr = xhr.target;
        if (xhr.readyState == 4 && xhr.status < 400) {
          try {
            var data = JSON.parse(xhr.responseText);
            if (!data.valid) {
              $('#mail-errors').html(data.message.join('<br />'));
              enableSubmit(false);
            }
            else {
              $('#mail-errors').html('This email is available.');
              enableSubmit(true);
            }
          }
          catch (e) {
            console.log(e);
          }
        }
      };
      xhr.send();
      $('#mail-errors').html('<div class="ajax-progress ajax-progress-throbber"><div class="throbber">&nbsp;</div></div>');
    }
  }

})(jQuery);
