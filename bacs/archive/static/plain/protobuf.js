$(document).ready(function() {
    $('.protobuf .spoiler').click(function() {
        $(this).parent().find('pre').toggle();
        $(this).find('.action').toggle();
    });
});
