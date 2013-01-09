$(function() {
	var first_time_records = [],
		second_time_records = [],
		third_time_records = [];

	function graph_three_time_delay() {
		nv.addGraph(function() {
			var chart = nv.models.lineChart();

			chart.xAxis.axisLabel('Number').tickFormat(d3.format(',r'));
			chart.yAxis.axisLabel('Time (s)').tickFormat(d3.format('.03f'));

			d3.select('#time-statistics svg').datum(get_time_data()).transition().duration(500).call(chart);
			nv.utils.windowResize(function() {
				d3.select('#time-statistics svg').call(chart)
			});
			return chart;
		});

		function get_time_data() {
			return [{
				values: first_time_records,
				key: '基本控制'
			}, {
				values: second_time_records,
				key: '精确控制',
				color: '#000'
			}, {
				values: third_time_records,
				key: '批量指令'
			}];
		}
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
				var end_time = (new Date()).getTime();
				pos = data.indexOf('-');
				time_for_tries = 0;
				if(pos != -1){
					max_try = data.slice(pos+1);
					time_for_tries = parseInt(max_try) * 0.3;
					data = data.slice(0,pos);
				}
				$("#buttons-status").append(data);
				first_time_records.push({x:first_time_records.length + 1, y:(end_time - begin_time) / 1000 - time_for_tries});
				//$("#time-statistics svg").empty();
				graph_three_time_delay()
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
					var end_time = (new Date()).getTime(),
						time_for_tries = 0,
						pos = data.indexOf('-');
					if(pos != -1){
						max_try = data.slice(pos+1);
						time_for_tries = parseInt(max_try) * 0.3;
						data = data.slice(0,pos);
					}
					if(data !== "true") {
						$("#inputtexts-status").append(data);
					}
					ele.removeAttr("disabled");
					
					second_time_records.push({x:second_time_records.length + 1, y:(end_time - begin_time) / 1000 - time_for_tries});
					graph_three_time_delay()
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
					var end_time = (new Date()).getTime();
					pos = data.indexOf('-');
					time_for_tries = 0;
					if(pos != -1){
						max_try = data.slice(pos+1);
						time_for_tries = parseInt(max_try) * 0.3;
						data = data.slice(0,pos);
					}
					if(data !== "true") {
						$("#textarea-status").append(data);
					}
					ele.removeAttr("disabled");
					third_time_records.push({x: third_time_records.length + 1, y:(end_time - begin_time) / 1000});
					//$("#time-statistics svg").empty();
					graph_three_time_delay()
				});
			}
		}
	});
})