function ToggleImage (id)
{
	var imgElement = document.getElementById(id);
	
	if (imgElement)
	{
		if (!imgElement.src.match(/empty.gif$/))
		{
			if (imgElement.src.match(/open.gif$/))
			{
				imgElement.src = "images/closed.gif";
			}
			else
			{
				imgElement.src = "images/open.gif";
			}
		}
	}
}

function ToggleContents (id)
{
	var divElement = document.getElementById(id);
	
	if (divElement)
	{
		if (divElement.style.display == "block")
		{
			divElement.style.display = "none";
		}
		else
		{
			divElement.style.display = "block";
		}
	}
}