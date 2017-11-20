/*
  ==============================================================
  func.js - JavaScript library for OOD Project3 

  Yipei Zhu, CSE687 - Spring 2017
  ==============================================================

Maintenance History:
====================
ver 1.0 : 04 April 2017
- first release
*/


/* ----------------------------------------------------------
 * to expand or collapse class bodies
 */
function $use(targetid, targetid2, objN) {
    var d = document.getElementById(targetid);
    var d2 = document.getElementById(targetid2);
    var sb = document.getElementById(objN);
    if (d.style.display == "none") {
        d.style.display = "block";
        d2.style.display = "none";
        sb.innerHTML = '-';
    } else {
        d.style.display = "none";
        d2.style.left = sb.attributes["value"].value + "px";
        d2.style.marginTop = "-25px";
        d2.style.display = "block";
        sb.innerHTML = "+";

    }
}