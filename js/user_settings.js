(function ($) {

  Drupal.behaviors.userSettings = {
    attach: function(context) {
      if (this.processed) return;
      this.processed = true;

      var f = $.farbtastic('#color-picker');
      var selected;
      $('.color', context)
        .each(function() {
          f.linkTo(this);
        })
        .on('focus', function() {
          if (selected) {
            $(selected).parent('.form-item').removeClass('color-selected');
          }
          f.linkTo(this);
          $(selected = this).parent('.form-item').addClass('color-selected');
        });

    $('.select-font input')
      .each(function() {
        var t = $(this);
        t.siblings('label').css('font-family', t.val());
      });
    }
  };


}(jQuery));
