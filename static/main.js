$(function(){
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
		$(data).find('robot').each(function(){
			var $robot = $(this);
			var id = $robot.find('id').text();
			$("div#allrobots>ul").append("<li id='"+id+"'>"+id+"</id>");
			$("form#sendcmd>select").append("<option value='"+id+"'>"+id+"</option>");
			$("form#uploadform>select").append("<option value='"+id+"'>"+id+"</option>");
		});
	});
}); 

$("div#allrobots>ul>li").live('click',function(event){
	//alert($(this)[0].innerText);
	$.post( "#", { id: $(this)[0].innerText}, function(data){
		var id = $(data).find('id').text();
		var state = $(data).find('state').text();
		var power = $(data).find('power').text();
		$("div#onerobot>ul").empty().append("<li>ID:&nbsp;&nbsp;"+id+"</li><li>State:&nbsp;&nbsp;"+state+"</li><li>Power:&nbsp;&nbsp;"+power+"</li>");
	});
});

$("#uploadform").ajaxForm({
	beforeSubmit: function(){
		$("#submitinfo").html("Submitting...");
	},
	success: function(){
		$("#submitinfo").html("Submitted!");
	}
});
})
