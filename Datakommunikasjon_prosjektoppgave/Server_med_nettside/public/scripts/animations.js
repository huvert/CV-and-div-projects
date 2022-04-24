"use strict";

// ===          Animations          ===

var animations = {
  hide_all_windows: function(filter = null) {
    if (filter !== "data") {
      $("#window-data").slideUp(200);
    }
    if (filter !== "admin") {
      $("#window-admin").slideUp(200);
    }
    if (filter !== "log") {
      $("#window-log").slideUp(200);
    }
    if (filter !== "client-log") {
      $(".main-container").css("z-index","1");
      $(".client-panel").hide();
    }
  }
};


// ===  Navbar Links and Animations  ===
$(function() {          // Waits for document to fully load before executing any JS code
  $("#nav-logo").on("click", function() {
    animations.hide_all_windows();
    setTimeout(()=> {
      $(".main-container").css("z-index","3");
      $(".client-panel").slideDown(200);
    },300);
  });

  $("#nav-log").on("click", function() {
    animations.hide_all_windows("log");
    $("#window-log").slideDown(200);
  });

  $("#nav-data").on("click", function() {
    animations.hide_all_windows("data");
    $("#window-data").slideDown(200);
  });

  $("#nav-admin").on("click", function() {
    animations.hide_all_windows("admin");
    $("#rick-rolled").html('');
    $("#admin-container").show();
    $("#window-admin").slideDown(200);
  });

  // ===  Other buttons and animations  ===
  // Rick rolled
  $(".admin-btn").on("click", function() {
    console.log("HELLO");
    $("#admin-container").hide();
    $("#rick-rolled").html('<div class="iframe-container"><iframe src="https://www.youtube.com/embed/BBJa32lCaaY?autoplay=1&rel=0&controls=0&disablekb=1" width="560" height="315" frameborder="0" allow="autoplay"></iframe></div>');
  });
});
