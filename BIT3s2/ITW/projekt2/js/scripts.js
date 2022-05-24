var scrollCurrentPosition = 0;
var scrollCurrentPositionBlock = "start";
var scrollTimeout = null;
/*
document.addEventListener("wheel", function(event) {

    setTimeout(function() {
        if (Math.sign(event.deltaY) == 1) {// down
            if (document.getElementsByClassName("content_block")[scrollCurrentPosition].clientHeight > document.documentElement.clientHeight &&
            !(Math.round(document.getElementsByClassName("content_block")[scrollCurrentPosition].getBoundingClientRect().bottom) <= (window.innerHeight || document.documentElement.clientHeight))) {
                scrollCurrentPositionBlock = "end";
                document.getElementsByClassName("content_block")[scrollCurrentPosition].scrollIntoView({behavior: "smooth", block: "end"});
            } else {
                if (scrollCurrentPosition < document.getElementsByClassName("content_block").length-1) {
                    scrollCurrentPositionBlock = "start";
                    document.getElementsByClassName("content_block")[scrollCurrentPosition+1].scrollIntoView({behavior: "smooth"});
                }
            }
        } else {// up
            if (document.getElementsByClassName("content_block")[scrollCurrentPosition].clientHeight > document.documentElement.clientHeight &&
            !(Math.round(document.getElementsByClassName("content_block")[scrollCurrentPosition].getBoundingClientRect().top) >= 0)) {
                scrollCurrentPositionBlock = "start";
                document.getElementsByClassName("content_block")[scrollCurrentPosition].scrollIntoView({behavior: "smooth", block: "start"});
            } else {
                if (scrollCurrentPosition > 0) {
                    scrollCurrentPositionBlock = "start";
                    document.getElementsByClassName("content_block")[scrollCurrentPosition-1].scrollIntoView({behavior: "smooth"});
                }
            }
        }
        


        clearTimeout(scrollTimeout);
        scrollTimeout = setTimeout(function() {
            // scroll ended - finish scrolling
            
            if ((scrollCurrentPositionBlock == "start" && (Math.round(document.getElementsByClassName("content_block")[scrollCurrentPosition].getBoundingClientRect().top) != 0))
            || (scrollCurrentPositionBlock == "end" && (Math.round(document.getElementsByClassName("content_block")[scrollCurrentPosition].getBoundingClientRect().bottom) != (window.innerHeight || document.documentElement.clientHeight)))) {
                
                console.log(scrollCurrentPosition)
                console.log(scrollCurrentPositionBlock)
                document.getElementsByClassName("content_block")[scrollCurrentPosition].scrollIntoView({behavior: "smooth", block: scrollCurrentPositionBlock});
            }
        }, 1000);
    }, 100);



    // disable default scroll
    event.preventDefault();
    event.stopImmediatePropagation();

},{ passive: false }); // passive: false to use preventDefault();
*/



document.addEventListener("scroll", function(event) {
    updateSectionBar();
});



var loadBarInterval = null;
window.addEventListener("load", function(event) {

    updateSectionBar();

    document.getElementsByClassName("content_block")[scrollCurrentPosition].scrollIntoView({behavior: "smooth"});

    loadBarInterval = setInterval(function () {
        var currentState = Number(getComputedStyle(document.getElementsByClassName("loader_bar_border")[0]).getPropertyValue("--load"));

        if (currentState == 0) {
            document.getElementsByClassName("image-container")[0].classList.remove("image-selected-2");
            document.getElementsByClassName("image-container")[0].classList.remove("image-selected-3");
            document.getElementsByClassName("image-container")[0].classList.add("image-selected-1");
            /*document.getElementsByClassName("slider_text")[1].style.display = 'none';
            document.getElementsByClassName("slider_text")[2].style.display = 'none';
            document.getElementsByClassName("slider_text")[0].style.display = 'block';*/
        } else if (currentState == 50) {
            document.getElementsByClassName("image-container")[0].classList.remove("image-selected-1");
            document.getElementsByClassName("image-container")[0].classList.remove("image-selected-3");
            document.getElementsByClassName("image-container")[0].classList.add("image-selected-2");
            /*document.getElementsByClassName("slider_text")[0].style.display = 'none';
            document.getElementsByClassName("slider_text")[2].style.display = 'none';
            document.getElementsByClassName("slider_text")[1].style.display = 'block';*/
        } else if (currentState == 100) {
            document.getElementsByClassName("image-container")[0].classList.remove("image-selected-1");
            document.getElementsByClassName("image-container")[0].classList.remove("image-selected-2");
            document.getElementsByClassName("image-container")[0].classList.add("image-selected-3");
            /*document.getElementsByClassName("slider_text")[0].style.display = 'none';
            document.getElementsByClassName("slider_text")[1].style.display = 'none';
            document.getElementsByClassName("slider_text")[2].style.display = 'block';*/
        }

        if (currentState == 101) {
            clearInterval(loadBarInterval);
        } else {
            document.getElementsByClassName("loader_bar_border")[0].style.setProperty("--load", ++currentState);
        }
    }, 6000/100);

    Array.from(document.getElementById("sections_bar").children).forEach((el, index) => {
        el.addEventListener("click", function(event) {
            scrollCurrentPositionBlock = "start";
            document.getElementsByClassName("content_block")[index].scrollIntoView({behavior: "smooth"});
        });
    });

    document.getElementsByClassName("loader_bar_border")[0].children[0].addEventListener("click", function(event) {
        document.getElementsByClassName("image-container")[0].classList.remove("image-selected-2");
        document.getElementsByClassName("image-container")[0].classList.remove("image-selected-3");
        document.getElementsByClassName("image-container")[0].classList.add("image-selected-1");
        /*document.getElementsByClassName("slider_text")[1].style.display = 'none';
        document.getElementsByClassName("slider_text")[2].style.display = 'none';
        document.getElementsByClassName("slider_text")[0].style.display = 'block';*/
    });
    document.getElementsByClassName("loader_bar_border")[0].children[1].addEventListener("click", function(event) {
        document.getElementsByClassName("image-container")[0].classList.remove("image-selected-1");
        document.getElementsByClassName("image-container")[0].classList.remove("image-selected-3");
        document.getElementsByClassName("image-container")[0].classList.add("image-selected-2");
        /*document.getElementsByClassName("slider_text")[0].style.display = 'none';
        document.getElementsByClassName("slider_text")[2].style.display = 'none';
        document.getElementsByClassName("slider_text")[1].style.display = 'block';*/
    });
    document.getElementsByClassName("loader_bar_border")[0].children[2].addEventListener("click", function(event) {
        document.getElementsByClassName("image-container")[0].classList.remove("image-selected-1");
        document.getElementsByClassName("image-container")[0].classList.remove("image-selected-2");
        document.getElementsByClassName("image-container")[0].classList.add("image-selected-3");
        /*document.getElementsByClassName("slider_text")[0].style.display = 'none';
        document.getElementsByClassName("slider_text")[1].style.display = 'none';
        document.getElementsByClassName("slider_text")[2].style.display = 'block';*/
    });
    
    document.getElementById("buttonhover").addEventListener("click", function(event) {
        scrollCurrentPositionBlock = "start";
        document.getElementsByClassName("content_block")[1].scrollIntoView({behavior: "smooth"});
        /*scrollCurrentPosition = 1;*/
    });

    document.getElementById("section_about").getElementsByTagName("button")[0].addEventListener("click", function(event) {
        scrollCurrentPositionBlock = "start";
        document.getElementsByClassName("content_block")[2].scrollIntoView({behavior: "smooth"});
        /*scrollCurrentPosition = 2;*/
    });

    document.getElementById("section_services").getElementsByTagName("button")[0].addEventListener("click", function(event) {
        scrollCurrentPositionBlock = "start";
        document.getElementsByClassName("content_block")[3].scrollIntoView({behavior: "smooth"});
        /*scrollCurrentPosition = 3;*/
    });

});

function updateSectionBar()
{
    const currentContentBlock = document.elementFromPoint(0,document.documentElement.clientHeight/2).closest(".content_block");
    const currentContentIndex = Array.from(document.getElementsByClassName("content_block")).indexOf(currentContentBlock);

    Array.from(document.getElementById("sections_bar").children).forEach((el) => {el.className = "";});
    document.getElementById("sections_bar").children[currentContentIndex].className = "section_selected";

    scrollCurrentPosition = currentContentIndex;
}
