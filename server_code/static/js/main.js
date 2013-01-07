$(function() {
	var time_records = [];

	function graph_time_delay(container) {

		var
		start = 0,
			options, graph, i, x, o;

		options = {
			xaxis: {
				mode: "int",
				labelsAngle: 45
			},
			selection: {
				mode: 'x'
			},
			HtmlText: false,
			title: 'Time-Delay'
		};

		// Draw graph with default options, overwriting with passed options

		function drawGraph(opts) {

			// Clone the options, so the 'options' variable always keeps intact.
			o = Flotr._.extend(Flotr._.clone(options), opts || {});

			// Return a new graph.
			return Flotr.draw(
			container, [time_records], o);
		}

		graph = drawGraph();

		Flotr.EventAdapter.observe(container, 'flotr:select', function(area) {
			// Draw selected area
			graph = drawGraph({
				xaxis: {
					min: area.x1,
					max: area.x2,
					mode: 'int',
					labelsAngle: 45
				},
				yaxis: {
					min: area.y1,
					max: area.y2
				}
			});
		});

		// When graph is clicked, draw the graph with default area.
		Flotr.EventAdapter.observe(container, 'flotr:click', function() {
			graph = drawGraph();
		});
	}

	$("button").live('click', function(event) {
		var ele = $(this);
		var cmd_array = new Array("info", "run", "forward", "backward", "turnLeft", "turnRight", "stop", "count");

		function contain(array, data) {
			for(element in array) {
				if(data === array[element]) return true;
			}
			return false;
		}
		var id = $(this).attr("id");
		if(contain(cmd_array, id)) {
			$(this).attr("disabled", "disabled");
			$("#buttons-status").empty();
			var begin_time = (new Date()).getTime();
			$.post("/", {
				command: id,
				towho: 'x'
			}, function(data) {
				if(id === "run") {
					$("#stop").removeAttr("disabled");
				} else if(id === "stop") {
					$("#run").removeAttr("disabled");
				} else {
					ele.removeAttr("disabled");
				}
				$("#buttons-status").append(data);
				var end_time = (new Date()).getTime();
				time_records.push([time_records.length + 1, (end_time - begin_time) / 1000]);
				graph_time_delay(document.getElementById("time-statistics"));
			});
		} else if(id === "all") {
			$("#targetnum").addClass("hidden");
			$(this).addClass("btn-inverse");
			$("#one").removeClass("btn-inverse");
		} else if(id === "one") {
			$("#targetnum").removeClass("hidden");
			$(this).addClass("btn-inverse");
			$("#all").removeClass("btn-inverse");
		} else if(id === "senddetailcmd") {
			$(this).attr("disabled", "disabled");
			$("#inputtexts-status").empty();
			var cmd = $("#cmd").val();
			var arguments = $("#distance").val();
			var targetnum = '0';
			if(!$("#targetnum").hasClass("hidden")) {
				targetnum = $("#targetnum").val();
			} else {
				targetnum = 'x';
			}
			if(cmd === "请选择指令" || targetnum === "请选择小车") {
				alert("你选择指令和小车了么？");
				ele.removeAttr("disabled");
			} else {
				if(targetnum == 'x') {
					if(cmd == 'run') {
						$("#stop").removeAttr("disabled");
						$("#run").attr("disabled", "disabled");
					} else if(cmd == 'stop') {
						$("#run").removeAttr("disabled");
						$("#stop").attr("disabled", "disabled");
					}
				}
				if(arguments === "") arguments = " ";
				var begin_time = (new Date()).getTime();
				$.post("/handledetailcmd", {
					command: cmd,
					args: arguments,
					target: targetnum
				}, function(data) {
					if(data !== "true") {
						$("#inputtexts-status").append(data);
					}
					ele.removeAttr("disabled");
					var end_time = (new Date()).getTime();
					time_records.push([time_records.length + 1, (end_time - begin_time) / 1000]);
					graph_time_delay(document.getElementById("time-statistics"));
				});
			}

		} else if(id === "commitpiliangcmd") {
			var ele = $(this);
			$(this).attr("disabled", "disabled");
			$("#textarea-status").empty();
			var multi_cmds = $("#multicmds").val().trim();
			if(multi_cmds === "") {
				alert("输入不能为空");
				ele.removeAttr("disabled");
			} else {
				var begin_time = (new Date()).getTime();
				$.post("/handlemulticmds", {
					multicmds: multi_cmds
				}, function(data) {
					if(data !== "true") {
						$("#textarea-status").append(data);
					}
					ele.removeAttr("disabled");
					var end_time = (new Date()).getTime();
					time_records.push([time_records.length + 1, (end_time - begin_time) / 1000]);
					graph_time_delay(document.getElementById("time-statistics"));
				});
			}
		}
	});
})