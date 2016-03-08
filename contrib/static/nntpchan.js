//
// nntpchan.js -- frontend ui niceness
//


// insert a backlink for a post given its short hash
function nntpchan_backlink(shorthash)
{
  var elem = document.getElementById("postform_message");
  if ( elem )
  {
    elem.value += ">>" + shorthash.substr(0,10) + "\n";
  }
}

var banner_count = 3;

// inject a banner into an element
function nntpchan_inject_banners(elem, prefix) {
  var n = Math.floor(Math.random() * banner_count);
  var banner = prefix + "static/banner_"+n+".jpg";
  var e = document.createElement("img");
  e.src = banner;
  e.id = "nntpchan_banner";
  elem.appendChild(e);
}

function get_storage() {
  var st = null;
  if (window.localStorage) {
    st = window.localStorage;
  } else if (localStorage) {
    st = localStorage;
  }
  return st;
}

function enable_theme(prefix, name) {

  var theme = document.getElementById("current_theme");
  if (theme) {
    theme.href = prefix + "static/"+ name + ".css";
    var st = get_storage();
    if (st.nntpchan === undefined) {
      st.nntpchan = {};
    }
    st.nntpchan.theme = name;
    st.nntpchan.prefix = prefix;
  }
}

document.onload = function() {
  var st = get_storage();
  if (st.nntpchan) {
    enable_theme(st.nntpchan.prefix, st.nntpchan.theme);
  }
}
