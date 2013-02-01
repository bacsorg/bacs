function update_archiver_format(index, type)
{
    parent = $(type);
    // does not allow infinite loop
    left = 100;
    do
    {
        parent = parent.parent();
        formats = parent.find("select[name='archiver_format']");
        --left;
    }
    while (left && !formats.length);
    formats.empty();
    switch (type.value)
    {
    case "tar":
        formats.append('<option value="">tar</option>');
        values = ["gzip", "bzip2", "xz", "lzma", "lzop"];
        break;
    case "7z":
        formats.append('<option value="">auto</option>');
        values = ["7z", "zip", "tar"];
        break;
    }
    for (i = 0; i < values.length; ++i)
        formats.append('<option value="' + values[i] + '">' + values[i] + '</option>');
}
