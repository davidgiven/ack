/* Simple Javascript tabs code.
 * Written in 2005 by David Given, dg@cowlark.com.
 * This code is in the public domain.
 *
 * $Source$
 * $State$
 */

/* All pages are hidden by default. */

document.write("<style type='text/css'>.page { display: none }</style>");

var page = function() {
	var s = document.location.toString();
	var i = s.indexOf("#");
	if (i == -1)
		return "home";
	return s.substr(i+1);
}()

function showpage(pagename)
{
	/* Hide old page. */
	
	var p = document.getElementById(page);
	p.style.display = "none";
	
	/* Show new page. */

	p = document.getElementById(pagename);
	p.style.display = "block";
	page = pagename;
}

/* Revision history
 * $Log$
 * Revision 1.2  2005/02/14 23:51:25  dtrg
 * Initial version.
 *
 * Revision 1.1  2005/02/14 23:39:07  dtrg
 * Initial version.
 */
