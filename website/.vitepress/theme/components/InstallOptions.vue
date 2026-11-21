<script setup lang="ts">
import { onMounted, ref } from 'vue'
const platforms = ['Linux & macOS', 'Windows']
const selected = ref(0)
function move(direction: number, event: KeyboardEvent) {
	selected.value =
		(selected.value + direction + platforms.length) % platforms.length
	;(event.currentTarget as HTMLElement).parentElement
		?.querySelectorAll('button')
		[selected.value].focus()
}
onMounted(() => {
	if (/Win/.test(navigator.platform)) selected.value = 1
})
</script>

<template>
	<div class="install-options" :data-platform="selected">
		<div class="install-tabs" role="tablist" aria-label="Operating system">
			<button
				v-for="(platform, index) in platforms"
				:id="`install-tab-${index}`"
				:key="platform"
				role="tab"
				:aria-selected="selected === index"
				:aria-controls="`install-panel-${index}`"
				:tabindex="selected === index ? 0 : -1"
				@click="selected = index"
				@keydown.right.prevent="move(1, $event)"
				@keydown.left.prevent="move(-1, $event)"
			>
				{{ platform }}
			</button>
		</div>
		<slot />
	</div>
</template>
