jQuery(document).ready(function($) {
	var nav=jQuery(".T_nav"),
	nav_li=jQuery(".T_nav li"),
	nav_a=jQuery(".T_nav li").children("a"),
	timer=[],
	isexpand=false,
	widths=[70,60,98,85,70],
	lefts=[150,110,80,15,-30],
	tops=[10,35,85,105,75];
	TP_obj={
		
		springDown:function(obj,index){
			obj.animate({top:"+="+(index*2+8)+"px"},(index+5)*80,function(){
				TP_obj.springUp(obj,index)
			})
		},
		springUp:function(obj,index){
			obj.animate({top:"-="+(index*2+8)+"px"},(index+5)*80,function(){
				TP_obj.springDown(obj,index)
			})
		},
		collapse:function(){
			for(var i=0; i<timer.length; i++){
				clearTimeout(timer[i]);
			}
			jQuery('#icon-animation').width(66).height(48).find('img').each(function(index,ele){
				if(index>0){
					index--;
					jQuery(ele).stop().animate({width:"0", height:"0",top:"25px",left:"15px"},200);
				}
			});
			isexpand = false;
		},
		expand:function(){
			jQuery('#icon-animation').width(250).height(200).find('img').each(function(index,ele){
				if(index>0){
					index--;
					jQuery(ele).stop().animate({width:widths[index]+"px", height:widths[index]+"px",top:tops[index]+"px",left:lefts[index]+"px"},600-(index*2+1)*60,function(){
						jQuery(this).animate({top:"-="+(index*2-1)*2+"px"},(index+1)*80,function(){
							if(index>2) TP_obj.springDown(jQuery(this),6-index-1)
							else TP_obj.springUp(jQuery(this),6-index-1)
						})
					});
				}
			});
			isexpand = true;
		},
		footPosition:function(){
			jQuery("#T_footer").attr("class","footer1")
		}
	};

	jQuery('#search_form').submit(function() {//搜索
		if(jQuery.trim(jQuery('#pn').val())=='') return false;
	});
	jQuery('#icon-animation').hover(function(){//鼠标经过跳动图标
		TP_obj.expand();
	},function(){
		TP_obj.collapse();
	});
	jQuery("#goto").live('click',function(){//回到顶部
		jQuery("html,body").animate({scrollTop:"0px"},"fast");
	})

	
$(".T_nav li").mouseenter(function(){//导航条效果鼠标经过
	var _this=$(this),_span=_this.children("span");
	if(!_this.hasClass("selected")&&!_span.is(":animated")){
		_this.addClass("hover")
		_span.css("width","14px")
		_span.animate({width:_this.width()+"px"},140,function(){
			$(this).animate({width:$(this).width()-14+"px"},30,function(){
				$(this).animate({width:$(this).width()+14+"px"},30,function(){
					if($(this).siblings("a").text()=="产品中心"){
						$(this).siblings("a").find("em").show();
						$("#products").show();
					}
				})
			})
		})
	}
}).mouseleave(function(){//导航条效果鼠标移出
	var _this=$(this);
	if(_this.children("a").text()=="产品中心"){
		$("#products").hide();
		_this.find("em").hide();
	}
	if(!$(this).hasClass("selected")){
		$(this).children("span").stop().animate({width:"14px"},80,function(){
			$(this).css("width","0px");
			_this.removeClass("hover")
		})
	}
})

jQuery(".V_icons li").live('mouseenter',function(){
	if(jQuery(this).attr("class")=="V_user_icon"||jQuery(this).attr("class")=="V_car_icon"){
		if(!isLogin){
			jQuery(this).append(login_str)
			jQuery(".V_jt,.V_login_icon",this).show();
		}else{
			jQuery(".V_jt,.V_icon_cont",this).show();
			jQuery(this).siblings().mouseleave();
		}
	}else{
		jQuery(".V_jt,.V_icon_cont",this).show();
		jQuery(this).siblings().mouseleave();
		if(jQuery(this).attr("class")=="V_pk_icon"){
			var _top,_contHeight=jQuery(".V_icon_cont",this).height(),_iconsHeight=jQuery(".V_icons").height();
			if(_contHeight-_iconsHeight>=0) _top=-240;
			else if(_contHeight>=190&&_contHeight<=_iconsHeight) _top=(_iconsHeight-_contHeight)-241
			else _top=0
			jQuery(".V_icon_cont",this).css("top",_top+"px")
		}

	}
    jQuery('.V_car_icon_cont').scrollTop(0);
	jQuery('.V_car_icon_cont').perfectScrollbar("update");
}).live('mouseleave',function(){
	jQuery(".V_jt,.V_icon_cont,.V_login_icon",this).hide();
	jQuery(".V_login_icon",this).remove();
})





	TP_obj.footPosition();
	//TP_obj.scrolling(jQuery(window))
	jQuery(window).resize(function(){
		TP_obj.footPosition();
		checkHeight()
		//TP_obj.scrolling(jQuery(window))
		alertPosition()
		//jQuery('.car_icon_cont').perfectScrollbar();
	}).scroll(function(){
		checkHeight()
		//TP_obj.scrolling(jQuery(this));
		TP_obj.footPosition();
		//checkHeight(jQuery(this).attr("class")=="car_icon",jQuery(".icon_cont",this),jQuery(window))

		//jQuery('.car_icon_cont').perfectScrollbar();
	})
	window.TP_obj=TP_obj;
    //feedback bug
    jQuery('#bug').click(function() {
        close_feedback_box();
        if (jQuery(this).attr('data-lenovo')) {
            jQuery('<div onclick="close_feedback_box()"></div>').addClass('bg_50').appendTo(jQuery('body'));
        var _box = jQuery('<div></div>').addClass('feedback_box').appendTo(jQuery('body'));
        var _iswai = jQuery(this).data('iswai');
        _iswai = _iswai == 'yes' ? '?iswai=yes' : '';
        jQuery('<iframe frameborder="0" scrolling="no"></iframe>').css({width:662,height:550}).attr('src',_root+"feedback.html"+_iswai).appendTo(_box);
        }else {
            alertLoginWin('yes');
        }
    });

});

function checkHeight(){
if(jQuery(".V_car_icon_cont").height()>jQuery(window).height()-104){
		jQuery(".V_car_icon_cont").height(jQuery(window).height()-62).css("top","-104px")
}
if(jQuery(window).scrollTop()>=0&&jQuery(window).scrollTop()<=62){
	jQuery(".V_car_icon_cont").height(jQuery(window).height()-(62-jQuery(window).scrollTop())).css("top",-104-jQuery(window).scrollTop()+"px")
}else{
	jQuery(".V_car_icon_cont").height(jQuery(window).height()).css("top",-104-62+"px")
}
}

//创建alert及mask
function alert2(conts,states){
	var src;
	if(jQuery("#mask0").length==0){
		jQuery("body").append("<div id='mask0'/>")
	}
	jQuery("#mask0").show();
	if(jQuery(".alert_tip").length==0){
		var str='<div class="alert_tip"><i class="alert_close"></i><div class="clear"></div><div class="alert_cont"><div class="txt"></div></div></div>'
		jQuery("body").append(str)
	}
	var _alert=jQuery(".alert_tip"),_alert_cont=jQuery(".alert_cont")
	if(states==1){
		_alert_cont.removeClass("e1")
	}else{
		_alert_cont.addClass("e1")
	}
	_alert_cont.children(".txt").html(conts)

	_alert.show();
	_alert_cont.children(".txt").css("marginTop",(60-_alert_cont.children(".txt").height())/2+"px")
	alertPosition()
    var thr=arguments[2]
	jQuery(".alert_close").click(function(){
		jQuery("#mask0,.alert_tip").hide()
            if(thr) window.location.reload();
	})
}
function alertPosition(){
	if(jQuery(".alert_tip").is(":visible"))
		jQuery(".alert_tip").css({top:(jQuery(window).height()-jQuery(".alert_tip").height())/2+"px",left:(jQuery(window).width()-jQuery(".alert_tip").width())/2+"px"})
}

//弹出登录框
function alertLoginWin(){
    location.href = _root+'login/index.html?backUrl='+encodeURIComponent(location.href)
}
/*function alertLoginWin(_reload){
    jQuery('<div id="mask" onclick="closeLoginWin(\'\');"></div>').appendTo(jQuery('body'));
    var _box = jQuery('<div style="position:fixed;z-index:9999999;width:478px; height:542px; left:50%; top:50%; margin-left:-238px; margin-top:-269px;overflow:hidden;"></div>').addClass('ajax_login_box').appendTo(jQuery('body'));
    jQuery('<iframe frameborder="0" scrolling="no" allowtransparency="true"></iframe>').css({width:478,height:542,'z-index':'999999'}).attr('src',_root+"Login/ajaxLoginPage.html"+(_reload=='yes'?'?reload=yes':'')).appendTo(_box);
    jQuery('#mask').show();
}*/
//关闭ajax登陆框
function closeLoginWin(_reload) {
    jQuery('.ajax_login_box,#mask').remove();
    if(_reload == 'yes') location.reload();
}
function show_loading_sumit(_this) {
    var _this = _this.find('input:submit');
    var _width = _this.outerWidth();
    var _height = _this.outerHeight();
    var _top = _this.offset().top;
    var _left = _this.offset().left;
    var _loading = _this.attr('rel') ? _this.attr('rel') : 'loading';
    return jQuery('<div></div>').css({width:_width,height:_height,top:_top,left:_left,'text-align':'center','background-image':'url('+_root+'Public/images/'+_loading+'.gif)','background-color':'#fff','background-position':'center','background-repeat':'no-repeat',opacity:0.8,'position':'absolute','z-index':'9999999'}).appendTo(jQuery('body'));
}
function close_feedback_box(yes){
    document.domain = 'thinkworldshop.com.cn';
    jQuery('.feedback_box,.bg_50').remove();
    if(yes == 'yes') location.reload();
}

function createImageUrl(_id,_size) {
    return _server_image_url + 'img/' + _id + '/' + _size + '.jpg';
}

try {
    
}
catch (e) {
}
