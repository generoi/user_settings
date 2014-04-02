(function ($) {

  $(document).ready(function() {

    var f = $.farbtastic('#color-picker');
    var selected;
    $('.color')
      .each(function() {
        f.linkTo(this);
      })
      .focus(function() {
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
  });

}(jQuery));
