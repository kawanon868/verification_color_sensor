#include "ev3api.h"
#include "app.h"

#include "./include/Clock.h"
#include "./include/ColorSensor.h"

void main_task(intptr_t unused) {

	const int N_ITE = 1000; // 検証用の繰り返し回数.
	const int SLEEP_DURATION = 4; // 検証時のスリープ時間. ETロボコンおなじみの4[ms].

	/* new を使用したインスタンス生成 */
	ev3api::Clock *clock = new ev3api::Clock();
	ev3api::ColorSensor *colorSensor = new ev3api::ColorSensor(PORT_2);

	/* 初期化処理 */
	clock->reset();

	/* 検証1 : ev3api::ColorSensor.getBrightness() の検証 */
	{
		colorSensor->getBrightness(); // ウォーミングアップのために一度反射光を測定.
		clock->sleep(1000); // 自タスクを1000[ms]スリープ. ウォーミングアップ完了待ちのため.

		// 検証開始.
		int t1 = (int)clock->now(); // 繰り返し実行前のクロック時間 [ms].
		for (int i = 0; i < N_ITE; i++) {
			colorSensor->getBrightness(); // 反射光を測定.
			clock->sleep(SLEEP_DURATION);
		}
		int t2 = (int)clock->now(); // 繰り返し実行後のクロック時間 [ms].

		// 結果をタスクログに表示.
		printf("Test_1 : %5d [ms]\n", (t2 - t1));
	}

	/* 検証2 : ev3api::ColorSensor.getRawColor() の検証 */
	{
		rgb_raw_t trash_rgb;
		colorSensor->getRawColor(trash_rgb); // ウォーミングアップのために一度RGB Raw値を測定.
		clock->sleep(1000); // 自タスクを1000[ms]スリープ. ウォーミングアップ完了待ちのため.

		// 検証開始.
		int t1 = (int)clock->now(); // 繰り返し実行前のクロック時間 [ms].
		for (int i = 0; i < N_ITE; i++) {
			colorSensor->getRawColor(trash_rgb); // RGB Raw値を測定.
			clock->sleep(SLEEP_DURATION);
		}
		int t2 = (int)clock->now(); // 繰り返し実行後のクロック時間 [ms].

		// 結果をタスクログに表示.
		printf("Test_2 : %5d [ms]\n", (t2 - t1));
	}

	/* 検証3 : 上二つの関数を交互に使用する時の検証 */
	{
		rgb_raw_t trash_rgb;
		colorSensor->getBrightness(); // ウォーミングアップのために一度反射光を測定.
		clock->sleep(1000); // 自タスクを1000[ms]スリープ. ウォーミングアップ完了待ちのため.

		// 検証開始.
		int t1 = (int)clock->now(); // 繰り返し実行前のクロック時間 [ms].
		for (int i = 0; i < N_ITE; i++) {
			colorSensor->getBrightness(); // 反射光を測定.
			colorSensor->getRawColor(trash_rgb); // RGB Raw値を測定.
			clock->sleep(SLEEP_DURATION);
		}
		int t2 = (int)clock->now(); // 繰り返し実行後のクロック時間 [ms].

		// 結果をタスクログに表示.
		printf("Test_3 : %5d [ms]\n", (t2 - t1));
	}

	/* タスク終了処理 */
	delete(colorSensor);
	delete(clock);
	ext_tsk();
}
