$(function(){
	$("button").live('click', function(event){
		
		var ele = $(this);
		var cmd_array = new Array("info", "run", "forward", "backward", "turnLeft", "turnRight", "stop", "count");
		function contain(array, data){
			for(element in array)
			{
				if(data === array[element])
					return true;
			}
			return false;
		}
		var id = $(this).attr("id");
		if(contain(cmd_array, id)){
			$(this).attr("disabled", "disabled");
			$.post("/", {command: id, towho: 'x'}, function(data){
				if(data === "true"){
					if(id === "run"){
						$("#stop").removeAttr("disabled");
					}
					else if(id === "stop")
					{
						$("#run").removeAttr("disabled");
					}
					else{
						ele.removeAttr("disabled");
					}
				}
				else{
					$("#buttons-status").empty().append(data);
					ele.removeAttr('disabled');
				}
			});
		}
		else if(id === "all")
		{
			$("#targetnum").addClass("hidden");
			$(this).addClass("btn-inverse");
			$("#one").removeClass("btn-inverse");
		}
		else if(id === "one")
		{
			$("#targetnum").removeClass("hidden");
			$(this).addClass("btn-inverse");
			$("#all").removeClass("btn-inverse");
		}
		else if(id === "senddetailcmd")
		{
			$(this).attr("disabled", "disabled");
			var cmd = $("#cmd").val();
			var arguments = $("#distance").val();
			var targetnum = '0';
			if(! $("#targetnum").hasClass("hidden"))
			{
				targetnum = $("#targetnum").val();
			}
			else{
				targetnum = 'x';
			}
			if(cmd === "请选择指令" || targetnum === "请选择小车")
			{
				alert("你选择指令和小车了么？");
				ele.removeAttr("disabled");
			}
			else{
				
				if(arguments === "")
					arguments = " ";

				$.post("/handledetailcmd", {command: cmd, args: arguments, target: targetnum}, function(data){
					if(data !== "true"){
						$("#inputtexts-status").empty().append(data);
					}
					ele.removeAttr("disabled");
				});
			}

		}
		else if(id == "commitpiliangcmd")
		{
			var ele = $(this);
			$(this).attr("disabled", "disabled");
			var multi_cmds = $("#multicmds").val().trim();
			if(multi_cmds === ""){
				alert("输入不能为空");
				ele.removeAttr("disabled");
			}
			else{
				$.post("/handlemulticmds", {multicmds: multi_cmds}, function(data){
					if(data !== "true")
					{
						$("#textarea-status").empty().append(data);
					}
					ele.removeAttr("disabled");
				});
			}
		}

	});
/*
	$("#sendcmd").submit(function(event){
		event.preventDefault();
	var $form = $( this );
	cmd = $form.find( 'input[name="command"]' ).val(),
	who = $form.find( 'select[name="towho"]' ).val(),
	url = $form.attr( 'action' );
	$.post( url, { command: cmd, towho: who}, function(data){
		$("div#allrobots>ul").empty();
		$("form#sendcmd>select").empty().append("<option value='All'>All</option>");
		$("form#uploadform>select").empty().append("<option value='All'>All</option>");
		$("div#allrobots>ul").append("<li>" + data + "</li>");
	});
}); 

$("div#allrobots>ul>li").live('click',function(event){
	//alert($(this)[0].innerText);
	$.post( "/", { id: $(this)[0].innerText}, function(data){
		var id = $(data).find('id').text();
		var state = $(data).find('state').text();
		var power = $(data).find('power').text();
		$("div#onerobot>ul").empty().append("<li>ID:&nbsp;&nbsp;"+id+"</li><li>State:&nbsp;&nbsp;"+state+"</li><li>Power:&nbsp;&nbsp;"+power+"</li>");
	});
});
*/
})
