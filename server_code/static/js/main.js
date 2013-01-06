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
			$("#buttons-status").empty();
			$.post("/", {command: id, towho: 'x'}, function(data){
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
				$("#buttons-status").append(data);
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
			$("#inputtexts-status").empty();
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
				if(targetnum == 'x'){
					if(cmd == 'run'){
						$("#stop").removeAttr("disabled");
						$("#run").attr("disabled", "disabled");
					}
					else if(cmd == 'stop'){
						$("#run").removeAttr("disabled");
						$("#stop").attr("disabled", "disabled");
					}
				}
				if(arguments === "")
					arguments = " ";

				$.post("/handledetailcmd", {command: cmd, args: arguments, target: targetnum}, function(data){
					if(data !== "true"){
						$("#inputtexts-status").append(data);
					}
					ele.removeAttr("disabled");
				});
			}

		}
		else if(id === "commitpiliangcmd")
		{
			var ele = $(this);
			$(this).attr("disabled", "disabled");
			$("#textarea-status").empty();
			var multi_cmds = $("#multicmds").val().trim();
			if(multi_cmds === ""){
				alert("输入不能为空");
				ele.removeAttr("disabled");
			}
			else{
				$.post("/handlemulticmds", {multicmds: multi_cmds}, function(data){
					if(data !== "true")
					{
						$("#textarea-status").append(data);
					}
					ele.removeAttr("disabled");
				});
			}
		}

	});
})
