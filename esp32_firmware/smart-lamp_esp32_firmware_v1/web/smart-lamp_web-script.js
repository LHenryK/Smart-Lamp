window.onload = (event) =>
{
    let selectedOption = "%lMode%";
    switch (selectedOption)
    {
        case "color":
            document.getElementById("selM").value = 'color';
            break;

        case "mqtt":
            document.getElementById("selM").value = 'mqtt';
            break;

        case "party":
            document.getElementById("selM").value = 'party';
            break;

        case "ani":
            document.getElementById("selM").value = 'anim';
            break;

        default:
            break;
    }
};

function autoSub()
{
    this.form.submit();
    update();
}


function update()
{
    setTimeout(function () { document.location.reload(false); }, 500);
}