(function ($) {

  $(document).ready(function() {

    var f = $.farbtastic('#color-picker');
    var selected;
    $('.color')
      .each(function() { f.linkTo(this); })
      .focus(function() {
        if (selected) {
          $(selected).removeClass('color-selected');
        }
        f.linkTo(this);
        $(selected = this).addClass('color-selected');
      });

  });

}(jQuery));
