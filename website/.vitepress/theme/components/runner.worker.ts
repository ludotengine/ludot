/// <reference lib="webworker" />
self.onmessage = async (
	event: MessageEvent<{
		runtime: string
		code: string
		action: 'run' | 'check'
	}>,
) => {
	let output = ''
	let truncated = false
	const print = (line: string) => {
		if (output.length < 16000)
			output += `${String(line).slice(0, 16000 - output.length)}\n`
		else truncated = true
	}
	try {
		const { default: createLudot } = await import(
			/* @vite-ignore */ event.data.runtime
		)
		const runtime = await createLudot({ print, printErr: print })
		const result =
			event.data.action === 'check'
				? runtime.ccall(
						'checkScript',
						'string',
						['string', 'number'],
						[event.data.code, 1],
					)
				: runtime.ccall(
						'executeScript',
						'string',
						['string'],
						[event.data.code],
					)
		self.postMessage({
			output: output + (truncated ? '\nOutput limit reached.\n' : ''),
			error: result || '',
			checked: event.data.action === 'check',
		})
	} catch (error) {
		self.postMessage({
			output,
			error: error instanceof Error ? error.message : String(error),
		})
	}
}
