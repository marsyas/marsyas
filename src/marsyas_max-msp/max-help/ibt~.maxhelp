{
	"patcher" : 	{
		"fileversion" : 1,
		"rect" : [ 64.0, 75.0, 720.0, 639.0 ],
		"bglocked" : 0,
		"defrect" : [ 64.0, 75.0, 720.0, 639.0 ],
		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"toolbarvisible" : 1,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"boxes" : [ 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Based on: João Lobato Oliveira, Fabien Gouyon, Luis Gustavo Martins, Luís Paulo Reis,\n\"IBT: A Real-time Tempo and Beat Tracking System.\" ISMIR 2010: 291-296",
					"linecount" : 2,
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 95.0, 70.0, 508.0, 34.0 ],
					"id" : "obj-30",
					"fontname" : "Arial",
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_top_description[1]",
					"text" : "Licenced under the GPL - Copyright 2007 - as part of MARSYAS (marsyas.info)",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 286.0, 602.0, 418.0, 20.0 ],
					"id" : "obj-20",
					"fontname" : "Arial",
					"fontsize" : 11.595187
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_top_digest",
					"text" : "a Max/MSP external of the real-time beat tracker IBT (INESC Porto Beat Tracker), based on MARSYAS",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 20.0, 42.0, 680.0, 21.0 ],
					"id" : "obj-28",
					"fontname" : "Arial",
					"textcolor" : [ 0.93, 0.93, 0.97, 1.0 ],
					"frgb" : [ 0.93, 0.93, 0.97, 1.0 ],
					"fontsize" : 12.754705
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_top_title",
					"text" : "ibt~",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 20.0, 12.0, 680.0, 30.0 ],
					"id" : "obj-27",
					"fontname" : "Arial",
					"textcolor" : [ 0.93, 0.93, 0.97, 1.0 ],
					"frgb" : [ 0.93, 0.93, 0.97, 1.0 ],
					"fontface" : 3,
					"fontsize" : 20.871338
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Input: Audio signal in the form of sound file or microphone input\nOuput: Bangs on the beat\n\nUsage (no specific order):\nibt~ [@winSize #] [@hopSize #] [@inductionTime #] [@minBPM #] [@maxBPM #] [@stateRecovery] [@outPathName #]\n\nParameters [default]:\n- @winSize: processing buffer size for the FFT window [1024]\n- @hopSize: hop size for processing overlap [512]\n- @inductionTime: duration of tempo induction stage (sec) [5.0]\n- @minBPM: minimum tempo limit (BPM) [81 -> to prevent octave error]\n- @maxBPM: maximum tempo limit (BPM) [160 -> to prevent octave error]\n- @stateRecovery: activates state-recovery by re-inducing and resetting the system whenever tracking seems to be lost - ideal for streaming scenarios (0-true/1-false) [0-false]\n- @outPathName: file/path name for outputing .txt with beat-times and median tempo [don't output files]",
					"linecount" : 18,
					"numinlets" : 1,
					"numoutlets" : 0,
					"bgcolor" : [ 0.92549, 0.92549, 0.92549, 1.0 ],
					"patching_rect" : [ 301.0, 122.0, 403.0, 255.0 ],
					"id" : "obj-12",
					"fontname" : "Arial",
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Developed by the SMCGroup @ INESC Porto\n              website: smc.inescporto.pt\nImplementation by João Lobato Oliveira \n              contact: jmldso@gmail.com",
					"linecount" : 4,
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 367.0, 535.0, 261.0, 62.0 ],
					"id" : "obj-6",
					"fontname" : "Arial",
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "ezadc~",
					"numinlets" : 1,
					"numoutlets" : 2,
					"patching_rect" : [ 180.0, 218.0, 45.0, 45.0 ],
					"id" : "obj-13",
					"outlettype" : [ "signal", "signal" ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Load an audio file\n(or connect the adc~ object to the ibt~ object)",
					"linecount" : 2,
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 27.0, 123.0, 257.0, 34.0 ],
					"id" : "obj-17",
					"fontname" : "Arial",
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0",
					"numinlets" : 2,
					"numoutlets" : 1,
					"patching_rect" : [ 42.0, 173.0, 32.5, 18.0 ],
					"id" : "obj-11",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "1",
					"numinlets" : 2,
					"numoutlets" : 1,
					"patching_rect" : [ 207.0, 417.0, 32.5, 18.0 ],
					"id" : "obj-14",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"numinlets" : 1,
					"numoutlets" : 1,
					"patching_rect" : [ 425.0, 402.0, 131.0, 131.0 ],
					"id" : "obj-10",
					"outlettype" : [ "bang" ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "cycle~ 1000",
					"numinlets" : 2,
					"numoutlets" : 1,
					"patching_rect" : [ 110.0, 430.0, 75.0, 20.0 ],
					"id" : "obj-4",
					"fontname" : "Arial",
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "*~ 1.",
					"numinlets" : 2,
					"numoutlets" : 1,
					"patching_rect" : [ 110.0, 540.0, 36.0, 20.0 ],
					"id" : "obj-3",
					"fontname" : "Arial",
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "adsr~ 0 100 1 50 @maxsustain 0.",
					"numinlets" : 5,
					"numoutlets" : 4,
					"patching_rect" : [ 127.0, 502.0, 190.0, 20.0 ],
					"id" : "obj-2",
					"fontname" : "Arial",
					"outlettype" : [ "signal", "signal", "", "" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "ibt~ @winSize 1024 @hopSize 512 @inductionTime 5. @minBPM 81 @maxBPM 160 @outPathName /Users/user/ @stateRecovery 0",
					"linecount" : 8,
					"numinlets" : 1,
					"numoutlets" : 1,
					"patching_rect" : [ 121.0, 286.0, 116.0, 117.0 ],
					"id" : "obj-9",
					"fontname" : "Arial",
					"outlettype" : [ "bang" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "ezdac~",
					"numinlets" : 2,
					"numoutlets" : 0,
					"patching_rect" : [ 84.0, 576.0, 45.0, 45.0 ],
					"id" : "obj-8"
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "1",
					"numinlets" : 2,
					"numoutlets" : 1,
					"patching_rect" : [ 131.0, 173.0, 32.5, 18.0 ],
					"id" : "obj-7",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "sfplay~",
					"numinlets" : 2,
					"numoutlets" : 2,
					"patching_rect" : [ 84.0, 217.0, 49.0, 20.0 ],
					"id" : "obj-1",
					"fontname" : "Arial",
					"outlettype" : [ "signal", "bang" ],
					"fontsize" : 12.0,
					"save" : [ "#N", "sfplay~", "", 1, 120960, 0, "", ";" ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "open",
					"numinlets" : 2,
					"numoutlets" : 1,
					"patching_rect" : [ 84.0, 173.0, 37.0, 18.0 ],
					"id" : "obj-5",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "panel",
					"varname" : "autohelp_top_panel",
					"numinlets" : 1,
					"background" : 1,
					"grad1" : [ 0.101961, 0.101961, 0.101961, 1.0 ],
					"numoutlets" : 0,
					"patching_rect" : [ 15.0, 12.0, 690.0, 52.0 ],
					"id" : "obj-15",
					"grad2" : [ 0.85, 0.85, 0.85, 1.0 ],
					"mode" : 1
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"source" : [ "obj-1", 0 ],
					"destination" : [ "obj-9", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-9", 0 ],
					"destination" : [ "obj-10", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-9", 0 ],
					"destination" : [ "obj-14", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-2", 0 ],
					"destination" : [ "obj-3", 1 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-14", 0 ],
					"destination" : [ "obj-2", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-4", 0 ],
					"destination" : [ "obj-3", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 0 ],
					"destination" : [ "obj-8", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-5", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-3", 0 ],
					"destination" : [ "obj-8", 1 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-11", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
 ]
	}

}
